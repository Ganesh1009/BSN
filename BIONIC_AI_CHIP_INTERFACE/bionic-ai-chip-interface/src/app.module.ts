import { Module } from '@nestjs/common';
import { ConfigModule } from '@nestjs/config';
import { AppController } from './app.controller';
import { AppService } from './app.service';
import { SessionsModule } from './sessions/sessions.module';
import { DeviceModule } from './device/device.module';

@Module({
    imports: [
        ConfigModule.forRoot({
            envFilePath: '.linux.home.env',
            isGlobal: true,
            expandVariables: true
        }),
        SessionsModule,
        DeviceModule],
    controllers: [AppController],
    providers: [AppService],
})
export class AppModule { }
