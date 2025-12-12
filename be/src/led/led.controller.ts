import { Body, Controller, Delete, Get, Param, Post, Put, UseGuards } from "@nestjs/common";
import { LedDto } from "./dto/led.dto";
import { LedService } from "./led.service";
import { AuthGuard } from "@nestjs/passport";

@Controller('/leds')
export class LedController {
    constructor(
        private readonly ledService: LedService,
    ) {}
    //@UseGuards(AuthGuard('jwt'))
    @Post()
    async createLed(@Body()body: LedDto): Promise<LedDto> {
        return this.ledService.createLed(body);
    }

    //@UseGuards(AuthGuard('jwt'))
    @Get()
    async getAllLed(): Promise<LedDto[]> {
        return await this.ledService.getAllLed();
    }

    //@UseGuards(AuthGuard('jwt'))
    @Put('/:id')
    async updateLed(
        @Body() body: LedDto,
        @Param() id: number
    ) {
        return await this.ledService.updateLed(body,id);
    }
    
    //@UseGuards(AuthGuard('jwt'))
    @Delete('/:id')
    async deleteLed(
        @Param() id: number
    ) {
        return await this.ledService.deleteLed(id);
    }
}