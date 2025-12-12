import { Injectable, NotFoundException } from "@nestjs/common";
import { EventEmitter2 } from "@nestjs/event-emitter";
import { InfluxService } from "src/influx/influx.service";
import { MqttService } from "src/mqtt/mqtt.service";
import { PrismaService } from "src/prisma/prisma.service";
import { EventEmitter } from "stream";
import { DeviceDto } from "./dto/device.dto";
import { consoleLogger } from "@influxdata/influxdb-client";
import { LedService } from "src/led/led.service";
import { LedDto } from "src/led/dto/led.dto";
import { LedControlDto } from "src/led/dto/ledControl.dto";

@Injectable()
export class DeviceService {
    constructor(
        private readonly influxService: InfluxService,
        private readonly prismaService: PrismaService,
        private readonly eventEmitter: EventEmitter2,
        private readonly ledService: LedService
    ) {}

    async getDeviceData(deviceId: string, durationMinutes: number, aggregateSeconds: number) {
        const rs = await this.influxService.getSensorHistory(deviceId, durationMinutes, aggregateSeconds);
        return rs;
    }

    async createDevice(deviceId: string, accountId?: number, name: string = "My device", nameLed: string = "My Led") {
        const data: any = { deviceId, name }; 
        if (accountId !== undefined) {
            data.accountId = accountId; 
        }
        const device = await this.prismaService.device.create({
            data,
        });
        const ledBody: LedDto = {
            name: nameLed,
            deviceId: device.id,
            color: "#FF0000"
        };
        const led = this.ledService.createLed(ledBody);
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

    async controlLed(body: LedControlDto, device_id: string) {
        this.eventEmitter.emit('led.controled', { device_id, body });
        const device = await this.prismaService.device.findUnique({
            where: { deviceId: device_id },
        });

        if (!device) {
            throw new NotFoundException("Device not found");
        }
        return this.prismaService.led.update({
            where: {
                deviceId: device.id,
            },
            data: {
                ...body,
            },
        });
    }

}