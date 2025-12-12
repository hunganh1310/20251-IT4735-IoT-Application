import { Body, Controller, Post } from "@nestjs/common";
import { LedDto } from "./dto/led.dto";
import { LedService } from "./led.service";

@Controller('/led')
export class LedController {
    constructor(
        private readonly ledService: LedService,
    ) {}
    @Post()
    async createLed(@Body()body: LedDto): Promise<LedDto> {
        return this.ledService.createLed(body);
    }
}