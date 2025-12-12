import { Injectable } from "@nestjs/common";
import { PrismaService } from "src/prisma/prisma.service";
import { LedDto } from "./dto/led.dto";

@Injectable()
export class LedService {
    constructor(
        private readonly prisma: PrismaService
    ) {}

    async createLed(body: LedDto): Promise<LedDto> {
        return await this.prisma.led.create({
            data: {
                ...body
            }
        });
    }
}