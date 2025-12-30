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

    async getAllLed(): Promise<LedDto[]> {
        return await this.prisma.led.findMany();
    }

    async updateLed(body: LedDto, id: number) {
        const { deviceId, ...updateData } = body;
        return await this.prisma.led.update({
            where: {
                id: Number(id),
            },
            data: {
                ...updateData
            }
        });
    }

    async deleteLed(id: number) {
        return this.prisma.led.delete({
            where: { id: Number(id) }
        });
    }
}