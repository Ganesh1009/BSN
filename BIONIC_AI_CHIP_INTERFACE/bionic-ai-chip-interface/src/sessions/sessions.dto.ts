import { ApiProperty } from '@nestjs/swagger';
import {
    IsNotEmpty,
    IsBoolean,
    IsNumber,
    IsString,
    IsOptional,
    Matches,
} from 'class-validator';

export class FactorySessions {
    public createSessionConfig(): SessionConfig {
        return {
            height: 175,
            weight: 75,
            isFemale: false,
            task: 'test-task',
        };
    }
}

export class SessionStatus {
    @IsNotEmpty()
    @IsBoolean()
    @ApiProperty({
        description: 'is there an active session right now?',
        type: Boolean,
        default: false,
    })
    isActive: boolean;

    @IsNumber()
    @IsOptional()
    @ApiProperty({
        description: 'session number',
        type: Number,
    })
    session?: number;
}

export class SessionData {
    @IsNotEmpty()
    @IsNumber()
    @ApiProperty({
        description: 'here its missing still',
        default: 1.75,
    })
    height: number;
}

export class SessionConfig {
    @IsNotEmpty()
    @IsNumber()
    @ApiProperty({
        description: 'height',
        default: 1.75,
    })
    height: number;

    @IsNotEmpty()
    @IsNumber()
    @ApiProperty({
        description: 'weight',
        default: 65,
    })
    weight: number;

    @IsNotEmpty()
    @IsBoolean()
    @ApiProperty({
        description: 'isFemale',
        default: true,
    })
    isFemale: boolean;

    @IsNotEmpty()
    @IsString()
    @ApiProperty({
        description: 'work session task',
        default: 'bricklayer',
    })
    task: string;
}

export class SessionSimulationConfig extends SessionConfig {
    @IsNumber()
    @IsNotEmpty()
    @ApiProperty({
        description: 'session start date'
    })
    /*readonly*/ start_date!: number
    @IsNumber()
    @IsNotEmpty()
    @ApiProperty({
        description: 'session stop date'
    })
    /*readonly*/ stop_date!: number

    @IsString()
    @Matches(/(?:UTC|[a-z]*?\/[a-z]+)/i, {
        message:
            'Only valid Olson TimeZones names are allowed, e.g. Europe/Berlin or UTC',
    })
    /*readonly*/ timezone!: string
}
