import { Injectable } from '@nestjs/common';
import { PrismaService } from 'src/prisma/prisma.service';
import { CreateAccountDto } from './dto/create-account.dto';
import * as bcrypt from 'bcrypt';

@Injectable()
export class AccountService {
    constructor(private prisma: PrismaService) {}
    
    async getAllAccounts() {
        return this.prisma.account.findMany();
    }

    async createAccount(accountData: CreateAccountDto) {
        const hashedPassword = await bcrypt.hash(accountData.password, 10);
        return this.prisma.account.create({
            data: {
                name: accountData.name,
                email: accountData.email,
                password: hashedPassword,
            }
        });
    }

    async updateAccount(id: number, accountData: CreateAccountDto) {
        return this.prisma.account.update({
            where: { id: id },
            data: {
                email: accountData.email,
                name: accountData.name,
                password: accountData.password,
            }
        });
    }

    async deleteAccount(id: number) {
        return this.prisma.account.delete({
            where: { id: id }
        });
    }
    
}