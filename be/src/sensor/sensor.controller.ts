import { Controller } from "@nestjs/common";
import { SensorService } from "./sensor.service";

@Controller('sensors')
export class SensorController {
    constructor(private readonly sensorService: SensorService) {}

    
}