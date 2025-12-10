import { Body, Controller, Delete, Get, Param, Post, Put, Query } from "@nestjs/common";
import { DeviceService } from "./device.service";
import { DeviceDto } from "./dto/device.dto";

@Controller('devices')
export class DeviceController {
    constructor(
        private readonly deviceService: DeviceService
    ) {}

    @Get('/:device_id/data')
    async getDeviceData(
        @Param('device_id') deviceId: string,
        @Query('durationMinutes') durationMinutes: number,
        @Query('aggregateSeconds') aggregateSeconds: number,
    ) {
        return await this.deviceService.getDeviceData(deviceId,durationMinutes,aggregateSeconds);
    }

    @Get()
    async getAllDevice() {
        return await this.deviceService.getAllDevice();
    }

    @Post()
    async createDevice(@Body() body: DeviceDto) {
        return await this.deviceService.createDevice(body.deviceId, body.accountId, body?.name);
    }

    @Put('/:id')
    async updateDevice(
        @Body() body: DeviceDto, 
        @Param('id') id: number,
        @Query('deviceId') deviceId: string
    ) {
        return await this.deviceService.updateDevice(body,id,deviceId);
    }

    @Delete('/:id')
    async deleteDevice(
        @Param('id') id: number,
        @Query('deviceId') deviceId: string
    ) {
        return await this.deviceService.deleteDevice(id, deviceId);
    }
}