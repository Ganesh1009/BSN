import {
    Controller,
    Optional,
    Get,
    Post,
    UsePipes,
    ValidationPipe,
    Body,
    Param,
    ParseIntPipe,
    Delete,
} from '@nestjs/common';
import { ApiTags, ApiOkResponse } from '@nestjs/swagger';
import { SessionsService } from './sessions.service';
import { SessionStatus, SessionConfig, SessionSimulationConfig } from './sessions.dto';
import { CreateBSNDataDTO } from './dto/create-bsndata.dto';

@ApiTags('sessions')
@Controller('sessions')
export class SessionsController {
    public constructor(@Optional() private sessionsService: SessionsService) {}

    @Get('status')
    @ApiOkResponse({
        description: 'The current Sessions status information object.',
        type: SessionStatus,
    })
    public getStatus(): SessionStatus {
        return this.sessionsService.getStatus();
    }

    @Get('config')
    @ApiOkResponse({
        description: 'The current Sessions Config information object.',
        type: SessionConfig,
    })
    public getConfig(): SessionConfig {
        return this.sessionsService.getConfig();
    }

    @Get()
    @ApiOkResponse({
        description: 'The current Sessions Config information object.',
        type: SessionConfig,
    })
    public getAllSessions(): number[] {
        return this.sessionsService.getSessions();
    }

    @Get(':id')
    @ApiOkResponse({
        description: 'The SessionData for this ID',
        type: CreateBSNDataDTO,
    })
    public getSession(
        @Param('id', new ParseIntPipe()) id: number,
    ): CreateBSNDataDTO {
        return this.sessionsService.getSession(id);
    }

    @Delete(':id')
    @ApiOkResponse({
        description: 'deleted session id',
        type: Number,
    })
    public deleteSession(
        @Param('id', new ParseIntPipe()) id: number,
    ): number {
        return this.sessionsService.deleteSession(id);
    }

    @Post('new')
    @UsePipes(
        new ValidationPipe({
            validationError: { target: false, value: false },
        }),
    )
    @ApiOkResponse({
        description:
            'update the current config wit the new config and starts a measurement',
        type: SessionConfig,
    })
    public newSession(@Body() newSession: SessionConfig): SessionConfig {
        return this.sessionsService.newSession(newSession);
    }

    @Post('stop')
    @UsePipes(
        new ValidationPipe({
            validationError: { target: false, value: false },
        }),
    )
    @ApiOkResponse({
        description: 'stops the active session',
        type: SessionStatus,
    })
    public stopSession(): SessionStatus {
        return this.sessionsService.stopSession();
    }


    @Post('simulate')
    @UsePipes(
        new ValidationPipe({
            validationError: { target: false, value: false },
        }),
    )
    @ApiOkResponse({
        description:
            'creates a faked session for the given time span between start and stop date',
        type: SessionSimulationConfig,
    })
    public simulateSession(@Body() newSession: SessionSimulationConfig): SessionConfig {
        return this.sessionsService.simulateSession(newSession);
    }
}
