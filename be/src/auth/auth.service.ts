import { Injectable } from "@nestjs/common";
import { JwtService } from "@nestjs/jwt";
import { PrismaService } from "src/prisma/prisma.service";
import * as bcrypt from 'bcrypt';

@Injectable()
export class AuthService {
    constructor(
        private jwtService: JwtService,
        private prisma: PrismaService   
    ) {}

    async validateUser(email: string, password: string): Promise<any> {
        const user = await this.prisma.account.findUnique({
            where: { email },
        });

        if (user && await bcrypt.compare(password, user.password)) {
            const { password, ...result } = user;
            console.log(password);
            return result;
        }
        return null;
    }

    async login(user: any) {
        const payload = { email: user.email }
        return {
            access_token: await this.jwtService.signAsync(payload),
            user: {
                id: user.id,
                email: user.email,
                name: user.name,
                role: user.role,
            },
        }
    }
}