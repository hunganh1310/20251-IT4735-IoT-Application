import { Injectable } from "@nestjs/common";
import { EventEmitter2 } from "@nestjs/event-emitter";
import { InfluxService } from "src/influx/influx.service";
import { MqttService } from "src/mqtt/mqtt.service";
import { PrismaService } from "src/prisma/prisma.service";
import { EventEmitter } from "stream";

@Injectable()
export class DeviceService {
    constructor(
        private readonly influxService: InfluxService,
        private readonly prismaService: PrismaService,
        private readonly eventEmitter: EventEmitter2,
    ) {}

    getDeviceData(deviceId: string) {
        return this.influxService.getLatestSensorData(deviceId);
    }

    async createDevice(deviceId: string, accountId?: number, name: string = "My device") {
        const data: any = { deviceId, name }; 
        if (accountId !== undefined) {
            data.accountId = accountId; 
        }
        const device = await this.prismaService.device.create({
            data,
        });
        await this.influxService.writeSensorData(deviceId);
        this.eventEmitter.emit('device.created', { deviceId });
        return device;
    }

    async getAllDeviceIds(): Promise<string[]> {
        const devices = await this.prismaService.device.findMany();
        return devices.map(device => device.deviceId);
    }
}