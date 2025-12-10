import { Injectable, Logger, OnModuleInit, OnModuleDestroy } from "@nestjs/common";
import { connect, IClientOptions, IClientPublishOptions, MqttClient } from "mqtt";
import { WebsocketGateway } from '../websocket/websocket.gateway';
import { randomUUID } from "crypto";
import { DeviceService } from "src/device/device.service";
import { OnEvent } from "@nestjs/event-emitter";
import { InfluxService } from "src/influx/influx.service";

@Injectable()
export class MqttService implements OnModuleInit, OnModuleDestroy {
    private client: MqttClient | null = null;
    private readonly logger = new Logger(MqttService.name);

    private ids: string[] = [];
    private topics: string[] = [];

    constructor(
        private readonly wsGateway: WebsocketGateway,
        private readonly deviceService: DeviceService,
        private readonly influxService: InfluxService
    ) {}

    private readonly uniqueIdPart = randomUUID().substring(0, 8);

    async onModuleInit() {
        this.ids = await this.deviceService.getAllDeviceIds();
        console.log(`IDS: ${this.ids}`);
        this.topics = this.ids.map(id => `iot/${id}/sensors`);

        this.connectBroker();
    }

    onModuleDestroy() {
        this.disconnectBroker();
    }

    connectBroker() {
        const url = "mqtts://990608f05d9049929317380e48de94d5.s1.eu.hivemq.cloud:8883";

        const options: IClientOptions = {
            clientId: `mqttx_test_01_${this.uniqueIdPart}`,
            clean: true,
            reconnectPeriod: 5000,
            connectTimeout: 30000,
            keepalive: 10,
            username: "mqtt1",
            password: "Mqtt123456",
            rejectUnauthorized: false, 
        };

        this.logger.log(`Connecting to MQTT broker ${url} ...`);

        this.client = connect(url, options);

        this.client.on('connect', () => {
            this.logger.log(`MQTT connected (clientId=${options.clientId})`);

            if (this.topics.length > 0) {
                this.client!.subscribe(this.topics, { qos: 0 }, (err, granted) => {
                    if (err) {
                        this.logger.error(`Subscribe error: ${err.message}`);
                    } else {
                        this.logger.log(`Subscribed: ${JSON.stringify(granted)}`);
                    }
                });
            }
        });

        this.client.on('message', (topic, message) => {
            const raw = message.toString();
            const deviceId = topic.split('/')[1]; 
            let parsed: any;
            try {
                parsed = JSON.parse(raw);
            } catch {
                parsed = raw;
            }
            // Validate payload
            if (!parsed) return;
            this.influxService.writeSensorData(deviceId,parsed.temperature,parsed.turbidity,parsed.water_quality,parsed.ph);
            this.wsGateway.sendSensorData({
                topic,
                payload: parsed,
                receivedAt: new Date().toISOString(),
            });
        });

        this.client.on('reconnect', () => this.logger.log('MQTT reconnecting...'));
        this.client.on('close', () => this.logger.log('MQTT connection closed'));
        this.client.on('offline', () => this.logger.log('MQTT offline'));
        this.client.on('error', (err) => this.logger.error(`MQTT error: ${err.message}`));
    }

    private disconnectBroker() {
        if (!this.client) return;

        this.logger.log('Disconnecting MQTT...');
        this.client.end(true, () => this.logger.log('MQTT disconnected'));
        this.client = null;
    }

    publish(topic: string, message: any, options: IClientPublishOptions = { qos: 0 }) {
        if (!this.client || !this.client.connected) {
            this.logger.warn('MQTT not connected, cannot publish');
            return;
        }

        const payload = typeof message === 'string' ? message : JSON.stringify(message);

        this.client.publish(topic, payload, options, (err) => {
            if (err) this.logger.error(`Publish error: ${err.message}`);
        });
    }

    @OnEvent('device.created')
    handleDeviceCreatedEvent(payload: { deviceId: string }) {
        const topic = `iot/${payload.deviceId}/sensors`;
        if (!this.topics.includes(topic)) {
            this.topics.push(topic);
        }
        if (this.client) {
            this.client.subscribe(topic, { qos: 0 }, (err) => {
                if (err) this.logger.error(`MQTT subscribe error: ${err.message}`);
                else this.logger.log(`Subscribed new device topic: ${topic}`);
            });
        }
    }

    @OnEvent('device.updated')
    handleDeviceUpdatedEvent(payload: { deviceId: string, newDeviceId: string }) {
        const oldTopic = `iot/${payload.deviceId}/sensors`;
        const newTopic = `iot/${payload.newDeviceId}/sensors`;
        const oldTopicIndex = this.topics.indexOf(oldTopic);
        if (oldTopicIndex > -1) {
            this.topics.splice(oldTopicIndex, 1);
        }

        if (this.client) {
            this.client.unsubscribe(oldTopic, (err) => {
                if (err) {
                    this.logger.error(`MQTT unsubscribe error for old topic ${oldTopic}: ${err.message}`);
                } else {
                    this.logger.log(`Unsubscribed old device topic: ${oldTopic}`);
                }
            });
        }
        if (!this.topics.includes(newTopic)) {
            this.topics.push(newTopic);
        }
        if (this.client) {
            this.client.subscribe(newTopic, { qos: 0 }, (err) => {
                if (err) {
                    this.logger.error(`MQTT subscribe error for new topic ${newTopic}: ${err.message}`);
                } else {
                    this.logger.log(`Subscribed new device topic: ${newTopic}`);
                }
            });
        }
    }

    @OnEvent('device.deleted') 
    handleDeviceDeletedEvent(payload: { deviceId: string }) {
        const topic = `iot/${payload.deviceId}/sensors`;
        const index = this.topics.indexOf(topic);
        if (index > -1) {
            this.topics.splice(index, 1);
        }
        if (this.client) {
            this.client.unsubscribe(topic, (err) => {
                if (err) {
                    this.logger.error(`MQTT unsubscribe error for topic ${topic}: ${err.message}`);
                } else {
                    this.logger.log(`Unsubscribed deleted device topic: ${topic}`);
                }
            });
        }
    }
}
