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
import { InfluxModule } from './influx/influx.module';
import { DeviceModule } from './device/device.module';
import { EventEmitterModule } from '@nestjs/event-emitter';
import { LedModule } from './led/led.module';

@Module({
  imports: [
    EventEmitterModule.forRoot(),
    AccountModule, 
    PrismaModule,  
    AuthModule, 
    WebsocketModule,
    MqttModule,
    InfluxModule,
    DeviceModule,
    LedModule
  ], 
  controllers: [AppController],
  providers: [AppService, MqttService, WebsocketGateway],
})
export class AppModule {}
