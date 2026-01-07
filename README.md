# IoT Application - Setup Guide

A comprehensive IoT application with NestJS backend, React frontend, and ESP32 firmware for monitoring and controlling IoT devices.

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [Project Structure](#project-structure)
- [Backend Setup](#backend-setup)
- [Frontend Setup](#frontend-setup)
- [Firmware Setup](#firmware-setup)
- [Running the Application](#running-the-application)
- [Troubleshooting](#troubleshooting)

## 🔧 Prerequisites

Before starting, ensure you have the following installed:

- **Node.js** (v18 or higher) - [Download](https://nodejs.org/)
- **npm** or **yarn** - Comes with Node.js
- **Docker** and **Docker Compose** - [Download](https://www.docker.com/products/docker-desktop/)
- **Git** - [Download](https://git-scm.com/)
- **PlatformIO** (for firmware) - [Installation Guide](https://platformio.org/install)

## 📁 Project Structure

```
20251-IT4735-IoT-Application/
├── be/              # Backend (NestJS)
├── FE/              # Frontend (React + Vite)
├── Firmware/        # ESP32 Firmware (PlatformIO)
├── MQTT_API.md      # MQTT API Documentation
└── README.md        # This file
```

## 🖥️ Backend Setup

The backend is built with NestJS and uses PostgreSQL, TimescaleDB, and InfluxDB for data storage.

### 1. Navigate to Backend Directory

```bash
cd be
```

### 2. Install Dependencies

```bash
npm install
```

### 3. Configure Environment Variables

Create a `.env` file in the `be` directory:

```env
# Database
DATABASE_URL="postgresql://postgres:dat11102004@localhost:5432/IotProject"

# JWT
JWT_SECRET="your-secret-key-here"

# MQTT
MQTT_BROKER_URL="mqtt://localhost:1883"
MQTT_USERNAME="your-mqtt-username"
MQTT_PASSWORD="your-mqtt-password"

# InfluxDB
INFLUXDB_URL="http://localhost:5086"
INFLUXDB_TOKEN="my-super-token"
INFLUXDB_ORG="my-org"
INFLUXDB_BUCKET="my-bucket"

# Server
PORT=3000
```

### 4. Start Database Services

Start PostgreSQL, TimescaleDB, and InfluxDB using Docker Compose:

```bash
docker-compose up -d
```

This will start:
- **PostgreSQL** on port `5432`
- **TimescaleDB** on port `5433`
- **InfluxDB** on port `5086`

### 5. Setup Prisma Database

Generate Prisma client and run migrations:

```bash
npx prisma generate
npx prisma migrate dev
```

### 6. Start Backend Server

**Development mode (with hot reload):**

```bash
npm run start:dev
```

**Production mode:**

```bash
npm run build
npm run start:prod
```

The backend API will be available at `http://localhost:3000`

### 7. Access Swagger API Documentation

Once the backend is running, visit:
- **Swagger UI**: `http://localhost:3000/api`

## 🎨 Frontend Setup

The frontend is built with React, TypeScript, Vite, and Material-UI.

### 1. Navigate to Frontend Directory

```bash
cd FE
```

### 2. Install Dependencies

```bash
npm install
```

### 3. Configure Environment Variables

Create a `.env` file in the `FE` directory:

```env
VITE_API_URL=http://localhost:3000
VITE_WS_URL=ws://localhost:3000
```

### 4. Start Development Server

```bash
npm run dev
```

The frontend will be available at `http://localhost:5173` (or another port if 5173 is busy)

### 5. Build for Production

```bash
npm run build
npm run preview
```

## 🔌 Firmware Setup

The firmware is for ESP32 devices using PlatformIO.

### 1. Navigate to Firmware Directory

```bash
cd Firmware
```

### 2. Configure Device Settings

Copy the example configuration:

```bash
cp include/config.example.h include/config.h
```

Edit `include/config.h` with your settings:

```cpp
// WiFi Configuration
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"

// MQTT Configuration
#define MQTT_SERVER "your-mqtt-broker-ip"
#define MQTT_PORT 1883
#define MQTT_USERNAME "your-mqtt-username"
#define MQTT_PASSWORD "your-mqtt-password"

// Device Configuration
#define DEVICE_ID "your-device-id"
```

### 3. Build and Upload

```bash
platformio run --target upload
```

### 4. Monitor Serial Output

```bash
platformio device monitor
```

## 🚀 Running the Application

### Complete Setup (All Services)

1. **Start Backend Services:**
   ```bash
   cd be
   docker-compose up -d
   npm run start:dev
   ```

2. **Start Frontend (in a new terminal):**
   ```bash
   cd FE
   npm run dev
   ```

3. **Flash Firmware (if needed):**
   ```bash
   cd Firmware
   platformio run --target upload
   ```

### Access the Application

- **Frontend**: `http://localhost:5173`
- **Backend API**: `http://localhost:3000`
- **Swagger Documentation**: `http://localhost:3000/api`
- **InfluxDB UI**: `http://localhost:5086`

### Default Credentials

**InfluxDB:**
- Username: `admin`
- Password: `admin123`

**PostgreSQL:**
- Username: `postgres`
- Password: `dat11102004`
- Database: `IotProject`

## 🛠️ Troubleshooting

### Backend Issues

**Port already in use:**
```bash
# Find and kill the process using port 3000
# Windows:
netstat -ano | findstr :3000
taskkill /PID <PID> /F

# Linux/Mac:
lsof -ti:3000 | xargs kill -9
```

**Database connection error:**
- Ensure Docker containers are running: `docker-compose ps`
- Check connection string in `.env` file
- Restart containers: `docker-compose restart`

**Prisma migration issues:**
```bash
npx prisma migrate reset  # Warning: This will delete all data
npx prisma migrate dev
```

### Frontend Issues

**Dependencies not found:**
```bash
rm -rf node_modules package-lock.json
npm install
```

**Build errors:**
```bash
npm run lint
npm run build
```

**Port already in use:**
- Vite will automatically use the next available port
- Or specify a port: `vite --port 5174`

### Docker Issues

**Containers not starting:**
```bash
docker-compose down
docker-compose up -d --force-recreate
```

**Check container logs:**
```bash
docker-compose logs -f [service-name]
# Example:
docker-compose logs -f postgres
docker-compose logs -f influxdb
```

**Clean up Docker resources:**
```bash
docker-compose down -v  # Removes volumes too
```

### Firmware Issues

**Upload failed:**
- Check USB connection
- Ensure correct COM port is selected
- Press BOOT button on ESP32 during upload

**Serial monitor not working:**
```bash
platformio device list  # List available devices
platformio device monitor --port /dev/ttyUSB0  # Specify port
```

## 📚 Additional Resources

- [NestJS Documentation](https://docs.nestjs.com/)
- [React Documentation](https://react.dev/)
- [Vite Documentation](https://vitejs.dev/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Prisma Documentation](https://www.prisma.io/docs)
- [MQTT API Documentation](./MQTT_API.md)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

## 📄 License

See [LICENSE](LICENSE) file for details.
