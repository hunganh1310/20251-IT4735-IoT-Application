import { Injectable } from '@nestjs/common';
import { InfluxDB, Point } from '@influxdata/influxdb-client';

@Injectable()
export class InfluxService {
  private influx: InfluxDB;
  private writeApi;

  private readonly url = 'http://localhost:8086';
  private readonly token = 'fGcdc1RtkIQNLWi22-kAxrVkaS5x6c5TikboVeLFE7R9aVuyz8_s_o6jYgfD2Cxp1Wgb5faJr7L1EhX20bx40Q==';
  private readonly org = 'iot-org';
  private readonly bucket = 'iot-sensor';

  constructor() {
    this.influx = new InfluxDB({ url: this.url, token: this.token });
    this.writeApi = this.influx.getWriteApi(this.org, this.bucket, 'ns');
  }

  async writeSensorData(deviceId: string, temperature: number = 0, turbidity: number = 0, water_quality: string = '', ph: number = 0) {
    const point = new Point('sensor')
      .tag('device_id', deviceId)
      .floatField('temperature', temperature)
      .floatField('turbidity', turbidity)
      .stringField('water_quality', water_quality)
      .floatField('ph', ph)
      .timestamp(new Date());

    this.writeApi.writePoint(point);

    try {
      await this.writeApi.flush();
    } catch (err) {
      console.error('InfluxDB write error:', err);
    }
  }

  async getLatestSensorData(deviceId: string) {
    const fluxQuery = `
        from(bucket: "${this.bucket}")
            |> range(start: -1h)
            |> filter(fn: (r) => r._measurement == "sensor")
            |> filter(fn: (r) => r.device_id == "${deviceId}")
            |> last()
    `;
    
  }

}
