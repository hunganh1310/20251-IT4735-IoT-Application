// websocket.gateway.ts
import { OnGatewayConnection, OnGatewayDisconnect, OnGatewayInit, WebSocketGateway, WebSocketServer } from '@nestjs/websockets';
import { Server } from 'socket.io';

@WebSocketGateway({
  cors: { origin: '*' }, // cho phép FE kết nối
})
export class WebsocketGateway implements OnGatewayInit, OnGatewayConnection, OnGatewayDisconnect{
  @WebSocketServer()
  server: Server;

  afterInit() {
    console.log(' WebSocket Gateway Initialized');
  }

  handleConnection(client: any) {
    console.log(` Client connected: ${client.id}`);
  }

  handleDisconnect(client: any) {
    console.log(` Client disconnected: ${client.id}`);
  }

  //ham nay duoc goi khi mqtt.service co du lieu moi
  sendSensorData(data: any) {
    console.log('Gửi dữ liệu tới FE:', data);
    this.server.emit('sensor_data', data);
  }
}
