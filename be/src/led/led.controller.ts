import { Body, Controller, Delete, Get, Param, Post, Put, UseGuards } from "@nestjs/common";
import { LedDto } from "./dto/led.dto";
import { LedService } from "./led.service";
import { AuthGuard } from "@nestjs/passport";
import { ApiBearerAuth } from "@nestjs/swagger";

@Controller('/leds')
export class LedController {
    constructor(
        private readonly ledService: LedService,
    ) {}
    @Post()
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    async createLed(@Body()body: LedDto): Promise<LedDto> {
        return this.ledService.createLed(body);
    }

    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    @Get()
    async getAllLed(): Promise<LedDto[]> {
        return await this.ledService.getAllLed();
    }

    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    @Put('/:id')
    async updateLed(
        @Body() body: LedDto,
        @Param() id: number
    ) {
        return await this.ledService.updateLed(body,id);
    }
    
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    @Delete('/:id')
    async deleteLed(
        @Param() id: number
    ) {
        return await this.ledService.deleteLed(id);
    }
}