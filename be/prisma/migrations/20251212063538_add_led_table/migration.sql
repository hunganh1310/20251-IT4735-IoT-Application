-- CreateTable
CREATE TABLE "Led" (
    "id" SERIAL NOT NULL,
    "name" TEXT NOT NULL,
    "createAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "deviceId" INTEGER NOT NULL,

    CONSTRAINT "Led_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "Led_deviceId_key" ON "Led"("deviceId");

-- AddForeignKey
ALTER TABLE "Device" ADD CONSTRAINT "Device_id_fkey" FOREIGN KEY ("id") REFERENCES "Led"("deviceId") ON DELETE RESTRICT ON UPDATE CASCADE;
