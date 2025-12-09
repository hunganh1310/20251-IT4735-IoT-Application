import { Injectable } from "@nestjs/common";
import { MqttService } from "src/mqtt/mqtt.service";

@Injectable()
export class SensorService {
    constructor(private readonly mqttService: MqttService) {}
}