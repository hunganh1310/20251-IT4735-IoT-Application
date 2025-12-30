-- AlterTable
ALTER TABLE "Led" ALTER COLUMN "led_is_on" SET DEFAULT false,
ALTER COLUMN "led_mode" SET DEFAULT 'basic',
ALTER COLUMN "presence_mode_enabled" SET DEFAULT false;
