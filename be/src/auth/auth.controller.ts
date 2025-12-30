import { Body, Controller, Post, UseGuards, Request, Get  } from "@nestjs/common";
import { AuthService } from "./auth.service";
import { AccountService } from "src/account/account.service";
import { CreateAccountDto } from "src/account/dto/create-account.dto";
import { AuthGuard } from "@nestjs/passport";
import { ApiBearerAuth, ApiBody, ApiOperation, ApiResponse, ApiTags } from "@nestjs/swagger";
import { LoginDto } from "./dto/login.dto";

@ApiTags('Auth')
@Controller('auth')
export class AuthController {
    constructor(
        private authService: AuthService,
        private accountService: AccountService
    ) {}

    @ApiOperation({ summary: 'Login to get JWT token' })
    @ApiResponse({ status: 200, description: 'Successful login returns JWT token.'})
    @ApiBody({ type: LoginDto})
    @UseGuards(AuthGuard('local'))  
    @Post('login')
    async login(@Request() req) {
        return this.authService.login(req.user);
    }

    @ApiResponse({status: 200, description: 'Sucessful register'})
    @ApiBody({type: CreateAccountDto})
    @Post('register')
    async register(@Body() body: CreateAccountDto) {
        return this.accountService.createAccount(body);
    }

    @ApiResponse({ status: 200, description: 'Get all accounts'})
    @ApiBearerAuth()
    @UseGuards(AuthGuard('jwt'))
    @Get('accounts')
    async getAllAccounts(@Request() req) {
        return this.accountService.getAllAccounts();
    }
}