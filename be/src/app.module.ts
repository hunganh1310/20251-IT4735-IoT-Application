import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { AppService } from './app.service';
import { AccountModule } from './account/account.module';
import { PrismaModule } from './prisma/prisma.module';
import { MqttService } from './mqtt/mqtt.service';
import { WebsocketGateway } from './websocket/websocket.gateway';
import { AuthModule } from './auth/auth.module';
import { WebsocketModule } from './websocket/websocket.module';
import { MqttModule } from './mqtt/mqtt.module';

@Module({
  imports: [
    AccountModule, 
    PrismaModule,  
    AuthModule, 
    WebsocketModule,
    MqttModule,
  ], 
  controllers: [AppController],
  providers: [AppService, MqttService, WebsocketGateway],
})
export class AppModule {}
