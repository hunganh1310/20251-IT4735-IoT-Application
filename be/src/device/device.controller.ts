import { Body, Controller, Delete, Get, Param, Patch, Post, Put, Query, UseGuards } from "@nestjs/common";
import { DeviceService } from "./device.service";
import { DeviceDto } from "./dto/device.dto";
import { LedDto } from "src/led/dto/led.dto";
import { LedControlDto } from "src/led/dto/ledControl.dto";
import { AuthGuard } from "@nestjs/passport";
import { ApiBearerAuth } from "@nestjs/swagger";

@Controller('devices')
export class DeviceController {
    constructor(
        private readonly deviceService: DeviceService
    ) {}

    //@UseGuards(AuthGuard('jwt'))
    @Get('/:device_id/data')
    async getDeviceData(
        @Param('device_id') deviceId: string,
        @Query('durationMinutes') durationMinutes: number,
        @Query('aggregateSeconds') aggregateSeconds: number,
    ) {
        return await this.deviceService.getDeviceData(deviceId,durationMinutes,aggregateSeconds);
    }

    //@UseGuards(AuthGuard('jwt'))
    @Patch('/:device_id/control')
    async controlLed(
        @Body() body: LedControlDto,
        @Param('device_id') device_id: string
    ) {
        return await this.deviceService.controlLed(body, device_id);
    }

    //@UseGuards(AuthGuard('jwt'))
    //@ApiBearerAuth()
    @Get()
    async getAllDevice() {
        return await this.deviceService.getAllDevice();
    }

    //@UseGuards(AuthGuard('jwt'))
    @Post()
    async createDevice(@Body() body: DeviceDto) {
        return await this.deviceService.createDevice(body.deviceId, body.accountId, body?.name, body.nameLed);
    }

    //@UseGuards(AuthGuard('jwt'))
    @Put('/:id')
    async updateDevice(
        @Body() body: DeviceDto, 
        @Param('id') id: number,
    ) {
        return await this.deviceService.updateDevice(body,id);
    }

    //@UseGuards(AuthGuard('jwt'))
    @Delete('/:id')
    //@ApiBearerAuth()
    async deleteDevice(
        @Param('id') id: number,
    ) {
        return await this.deviceService.deleteDevice(id);
    }
}