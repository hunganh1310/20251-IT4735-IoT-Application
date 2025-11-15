import { Injectable, Logger, OnModuleInit, OnModuleDestroy } from "@nestjs/common";
import { connect, IClientOptions, IClientPublishOptions, MqttClient } from "mqtt";
import { WebsocketGateway } from '../websocket/websocket.gateway';

@Injectable()
export class MqttService implements OnModuleInit, OnModuleDestroy{
    private client: MqttClient | null;
    private readonly logger = new Logger(MqttService.name);
    private readonly topics = "home/house1/livingroom/dht11/data";

    constructor(private readonly wsGateway: WebsocketGateway) {}

    onModuleInit() {
        this.connectBroker();
    }

    onModuleDestroy() {
        this.disconnectBroker();
    }

    connectBroker() {
        const url = "http://localhost";
        const options: IClientOptions = {
            clientId: "NestjsMqttClient",
            clean: true,
            reconnectPeriod: 5000,
            connectTimeout: 30_000,
            keepalive: 60,
            // username: process.env.MQTT_USERNAME,
            // password: process.env.MQTT_PASSWORD,
        }

        this.logger.log(`Connecting to MQTT broker ${url} ...`);

        this.client = connect(url, options);
        this.client.on('connect', (connack) => {
            this.logger.log(`MQTT connected (clientId=${options.clientId})`);
              
            this.client?.subscribe(this.topics, { qos: 0 }, (err, granted) => {
                if (err) {
                    this.logger.error(`Subscribe error for ${this.topics}: ${err.message}`);
                } else {
                    this.logger.log(`Subscribed to ${this.topics} (granted: ${JSON.stringify(granted)})`);
                }
            }); //subcribe topic
        });

        this.client.on('reconnect', () => this.logger.log('MQTT reconnecting...'));
        this.client.on('close', () => this.logger.log('MQTT connection closed'));
        this.client.on('offline', () => this.logger.log('MQTT offline'));
        this.client.on('error', (err) => this.logger.error('MQTT error: ' + err.message));

        this.client.on('message', (topic, message: Buffer) => {
            const payloadStr = message.toString();
            let parsed: any = payloadStr;
            try {
                parsed = JSON.parse(payloadStr);
            } catch(e) {
                this.logger.debug(`Payload is not JSON for topic ${topic}, using raw string`);
            }
            this.logger.debug(`Received message on ${topic}: ${payloadStr}`);
            

            // bat websocket
            try {
                // tùy nhu cầu: emit cả topic, raw payload, timestamp...
                this.wsGateway.sendSensorData({ topic, payload: parsed, receivedAt: new Date().toISOString() });
            } catch (e) {
                this.logger.error('Failed to emit websocket event: ' + (e as Error).message);
            }
        })
    }

    private disconnectBroker() {
        if(!this.client) return;
        this.logger.log('Disconnecting from MQTT broker...');
        try{
            this.client.end(true, () => this.logger.log('MQTT client disconnected'));
        } catch(e) {
            this.logger.error('Error while disconnecting MQTT client: ' + (e as Error).message);
        } finally {
            this.client = null;
        }
    }

    publish(topic: string, message: string | object, options: IClientPublishOptions  = { qos: 0, retain: false}) {
         if (!this.client || !this.client.connected) {
            this.logger.warn('MQTT not connected, cannot publish');
            return;
        }
        const payload = typeof message === 'string' ? message : JSON.stringify(message);
        this.client.publish(topic, payload, options, (err) => {
            if (err) {
                this.logger.error(`Publish error to ${topic}: ${err.message}`);
            } else {
                this.logger.log(`Published to ${topic}: ${payload}`);
            }
        });
    }
}