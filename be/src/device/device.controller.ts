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

    @Get('/:device_id/data')
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    async getDeviceData(
        @Param('device_id') deviceId: string,
        @Query('durationMinutes') durationMinutes: number,
        @Query('aggregateSeconds') aggregateSeconds: number,
    ) {
        return await this.deviceService.getDeviceData(deviceId,durationMinutes,aggregateSeconds);
    }

    @Patch('/:device_id/control')
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    async controlLed(
        @Body() body: LedControlDto,
        @Param('device_id') device_id: string
    ) {
        return await this.deviceService.controlLed(body, device_id);
    }

    @Get()
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    async getAllDevice() {
        return await this.deviceService.getAllDevice();
    }

    @Post()
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    async createDevice(@Body() body: DeviceDto) {
        return await this.deviceService.createDevice(body.deviceId, body.accountId, body?.name, body.nameLed);
    }

    @Put('/:id')
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    async updateDevice(
        @Body() body: DeviceDto, 
        @Param('id') id: number,
    ) {
        return await this.deviceService.updateDevice(body,id);
    }

    @Delete('/:id')
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    async deleteDevice(
        @Param('id') id: number,
    ) {
        return await this.deviceService.deleteDevice(id);
    }
}