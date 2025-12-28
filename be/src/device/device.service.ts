import { BadRequestException, Injectable, NotFoundException } from "@nestjs/common";
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
        if(!led) {
            throw new BadRequestException("Invalid input");
        }
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

    async updateDevice(body: DeviceDto, id: number) {
        const oldDevice = await this.prismaService.device.findUnique({ where: { id } });
        if(!oldDevice) {
            throw new NotFoundException("Device not found");
        }
        const updated = await this.prismaService.device.update({
            data: { ...body },
            where: { id }
        });

        if (oldDevice.deviceId !== body.deviceId) {
            this.eventEmitter.emit('device.updated', {
                oldDeviceId: oldDevice.deviceId,
                newDeviceId: body.deviceId
            });
        }
        return updated;
    }


    async deleteDevice(id: number): Promise<DeviceDto> {
        const device = await this.prismaService.device.findUnique({
            where: { id },
        });
        if (!device) {
            throw new NotFoundException("Device not found");
        }
        const led = await this.prismaService.led.findUnique({
            where: { deviceId: device.id }
        })
        if (!led) {
            throw new NotFoundException("Led not found");
        }
        const ledDeleted = await this.prismaService.led.delete({
            where: { deviceId: device.id }
        })
        const deleted = await this.prismaService.device.delete({
            where: { id },
        });
        this.eventEmitter.emit('device.deleted', {
            deviceId: device.deviceId,
        });

        return deleted;
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