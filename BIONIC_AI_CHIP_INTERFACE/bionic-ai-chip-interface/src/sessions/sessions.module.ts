import { Module } from '@nestjs/common';
import { SessionsController } from './sessions.controller';
import { SessionsService } from './sessions.service';
import { BsnClientService } from '../bsn-client/bsn-client.service';

@Module({
    controllers: [SessionsController],
    providers: [SessionsService, BsnClientService],
})
export class SessionsModule {}
