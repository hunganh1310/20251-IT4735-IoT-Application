import { Injectable } from '@nestjs/common';
import { flux, InfluxDB, Point } from '@influxdata/influxdb-client';

interface InfluxRow {
  _time: string;
  _field: string;
  _value: number;
  [key: string]: any;
}

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

  async getSensorHistory(deviceId: string, durationMinutes: number, aggregateSeconds: number) {
    const fluxQuery = `
      from(bucket: "${this.bucket}")
        |> range(start: -${durationMinutes}m)
        |> filter(fn: (r) => r._measurement == "sensor")
        |> filter(fn: (r) => r.device_id == "${deviceId}")
        |> filter(fn: (r) => r._field == "temperature" or r._field == "ph" or r._field == "turbidity" or r._field == "water_quality")
        |> aggregateWindow(every: ${aggregateSeconds}s, fn: mean)
        |> yield()
    `;

    const queryApi = this.influx.getQueryApi(this.org);
    const rows = await queryApi.collectRows(fluxQuery);
    console.log(`Rows: ${JSON.stringify(rows)}`);

    //  group các dòng theo _time
    const grouped: Record<string, any> = {};

    rows.forEach((row: InfluxRow) => {
      const time = row._time;
      const field = row._field;
      const value = row._value;

      if (!grouped[time]) grouped[time] = { time };
      grouped[time][field] = value;
    });

    // chuyển object → array
    return Object.values(grouped);
  }


}
