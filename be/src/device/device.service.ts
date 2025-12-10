import { Injectable } from "@nestjs/common";
import { EventEmitter2 } from "@nestjs/event-emitter";
import { InfluxService } from "src/influx/influx.service";
import { MqttService } from "src/mqtt/mqtt.service";
import { PrismaService } from "src/prisma/prisma.service";
import { EventEmitter } from "stream";
import { DeviceDto } from "./dto/device.dto";
import { consoleLogger } from "@influxdata/influxdb-client";

@Injectable()
export class DeviceService {
    constructor(
        private readonly influxService: InfluxService,
        private readonly prismaService: PrismaService,
        private readonly eventEmitter: EventEmitter2,
    ) {}

    async getDeviceData(deviceId: string, durationMinutes: number, aggregateSeconds: number) {
        const rs = await this.influxService.getSensorHistory(deviceId, durationMinutes, aggregateSeconds);
        return rs;
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

    async getAllDevice(): Promise<DeviceDto[]> {
        const devices = await this.prismaService.device.findMany();
        return devices;
    }

    async updateDevice(body: DeviceDto,id: number,deviceId: string): Promise<DeviceDto> {
        const data: DeviceDto = await this.prismaService.device.update({
            data: {
                ...body
            },
            where: {
                id: id
            }
        });
        const newDeviceId = body.deviceId;
        console.log(`DeviceId: ${deviceId} and Old deviceId: ${newDeviceId}`);
        if(deviceId != newDeviceId){
            this.eventEmitter.emit('device.updated', { deviceId, newDeviceId });
        }
        return data;
    }

    async deleteDevice(id: number, deviceId: string): Promise<DeviceDto> {
        const data = await this.prismaService.device.delete({
            where: {
                id: id
            }
        });
        this.eventEmitter.emit('device.deleted', { deviceId });
        return data;
    }
}