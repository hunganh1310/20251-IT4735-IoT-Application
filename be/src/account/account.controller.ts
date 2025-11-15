import { Body, Controller, Delete, Get, Param, Post, Put, UseGuards } from '@nestjs/common';
import { AccountService } from './account.service';
import { CreateAccountDto } from './dto/create-account.dto';
import { ApiBody, ApiOperation, ApiResponse } from '@nestjs/swagger';
import { AuthGuard } from '@nestjs/passport';

@Controller('accounts')
export class AccountController {
    constructor(private accountService: AccountService) {}
    @ApiOperation({ summary: 'Get all accounts' })
    @ApiResponse({ status: 200, description: 'List of all accounts' })
    @Get()
    async getAllAccounts() {
        return this.accountService.getAllAccounts();
    }

    @Post()
    async createAccount(@Body() createAccountDto: CreateAccountDto) {
        return this.accountService.createAccount(createAccountDto);
    }
    @Put(':id')
    async updateAccount(@Param('id') id: number, @Body() updateAccountDto: CreateAccountDto) {
        return this.accountService.updateAccount(Number(id), updateAccountDto);
    }

    @Delete(':id')
    async deleteAccount(@Param('id') id: number) {
        return this.accountService.deleteAccount(Number(id));
    }

}
