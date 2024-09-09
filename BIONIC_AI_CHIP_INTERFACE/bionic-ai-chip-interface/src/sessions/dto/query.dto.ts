import { ApiProperty } from '@nestjs/swagger'
import {
  IsMongoId,
  IsNumberString,
  IsOptional,
  IsString,
  Matches,
} from 'class-validator'
export class QueryDTO {
  @IsMongoId()
  @IsOptional()
  @ApiProperty({
    description:
      'The ID of the worker. This parameter is affected by security roles. It might have no effect depending on the privileges granted to the requester.',
  })
  readonly worker_id?: string

  @IsString()
  @IsOptional()
  @ApiProperty({ description: "The worker's session task." })
  readonly task?: string

  @IsString()
  @IsOptional()
  @ApiProperty({ description: 'The name of the data category.' })
  readonly category?: string

  @IsString()
  @IsOptional()
  @ApiProperty({ description: 'The name of the type of data.' })
  readonly data?: string

  @IsNumberString(
    { no_symbols: true },
    { message: 'level must be an integer >= 0.' }
  )
  @IsOptional()
  @ApiProperty({
    description: 'The level of the event with 0 being a GREEN event.',
  })
  readonly level?: string

  @IsOptional()
  @ApiProperty({
    description:
      'Can be specified either as number of milliseconds since Epochs (UNIX time) or as a date ISO8601 formatted.',
  })
  readonly date?: string

  @IsOptional()
  @IsString()
  @Matches(/(?:[a-z]*?\/[a-z]+|UTC)/i, {
    message:
      'timezone must be a valid Olson TimeZone name, e.g. Europe/Berlin or UTC.',
  })
  @ApiProperty({
    description:
      'The timezone where the session was recorded. Only valid Olson Timezone names, e.g. Europe/Berlin or UTC.',
  })
  readonly timezone?: string

  @IsOptional()
  @IsNumberString({ no_symbols: true })
  @Matches(/[0-9]{4}/i, { message: 'year must be a 4 digit number.' })
  @ApiProperty({ description: 'The year (4 digit).' })
  readonly year?: string

  @IsOptional()
  @IsNumberString({ no_symbols: true })
  @Matches(/\b([1-9]|1[0-2])\b/, {
    message: 'month must be between 1 and 12 inclusive.',
  })
  @ApiProperty({ description: 'The month of the year (1-12).' })
  readonly month?: string

  @IsOptional()
  @IsNumberString({ no_symbols: true })
  @Matches(/\b([1-9]|[12][0-9]|3[01])\b/, {
    message: 'day must be between 1 and 31 inclusive.',
  })
  @ApiProperty({ description: 'The day of the month (1-31).' })
  readonly day?: string

  @IsOptional()
  @IsNumberString({ no_symbols: true })
  @Matches(/[1-7]/, {
    message: 'isoDayOfWeek must be between 1 and 7 inclusive.',
  })
  @ApiProperty({ description: 'The day of the week (1-7 - 1 is Monday).' })
  readonly isoDayOfWeek?: string

  @IsOptional()
  @IsNumberString({ no_symbols: true })
  @Matches(/\b([0-9]|1[0-9]|2[0-3])\b/, {
    message: 'hour must be between 0 and 23 inclusive.',
  })
  @ApiProperty({ description: 'The hour of the day (0-23).' })
  readonly hour?: string

  @IsOptional()
  @IsNumberString(
    { no_symbols: true },
    { message: 'time must be an integer >= 0.' }
  )
  @ApiProperty({
    description:
      'This is the time an event occurred during a session. It is the number of milliseconds after the session started.',
  })
  readonly time?: string

  @IsOptional()
  @IsNumberString(
    { no_symbols: true },
    { message: 'limit must be an integer >= 0.' }
  )
  @ApiProperty({ description: 'Limit to # of results.' })
  readonly limit?: string

  @IsOptional()
  @IsNumberString(
    { no_symbols: true },
    { message: 'skip must be an integer >= 0.' }
  )
  @ApiProperty({ description: 'Skip to page #.' })
  readonly skip?: string
}
