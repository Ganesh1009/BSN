import { Type } from 'class-transformer'
import {
  IsArray,
  IsNotEmpty,
  IsNumber,
  IsOptional,
  IsString,
  Matches,
  ValidateNested,
} from 'class-validator'

import BSNCategory from '../interfaces/bsncategory.interface'
import BSNDatum from '../interfaces/bsndatum.interface'
import BSNEvent from '../interfaces/bsnevent.interface'
import BSNSession from '../interfaces/bsnsession.interface'

export class CreateDatum implements BSNDatum {
  @IsString()
  @IsNotEmpty()
  /*readonly*/ name!: string
  @IsNumber()
  @IsOptional()
  /*readonly*/ offset?: number
  @IsNumber()
  @IsOptional()
  /*readonly*/ sampling_rate?: number
  @IsNumber()
  @IsOptional()
  /*readonly*/ number_of_samples?: number
  // TODO distinguish between different data types using validators
  // https://github.com/typestack/class-transformer#providing-more-than-one-type-option
  // TODO distinguish between different data types using validators
  // https://github.com/typestack/class-transformer#providing-more-than-one-type-option
  @IsNotEmpty()
  /*readonly*/ samples!: Array<number[] | number | string> | number | string
}

export class CreateCategory implements BSNCategory {
  @IsString()
  @IsNotEmpty()
  /*readonly*/ name!: string
  @IsArray()
  @ValidateNested({ each: true })
  @Type(() => CreateDatum)
  /*readonly*/ data!: CreateDatum[]
}

export class CreateEvent implements BSNEvent {
  @IsNumber()
  @IsNotEmpty()
  /*readonly*/ time!: number
  @IsNumber()
  @IsNotEmpty()
  /*readonly*/ level!: number
  @IsArray()
  @ValidateNested({ each: true })
  @Type(() => CreateCategory)
  /*readonly*/ categories!: CreateCategory[]
}

export class CreateBSNDataDTO implements BSNSession {
  @IsString()
  @IsNotEmpty()
  /*readonly*/ task!: string
  @IsNumber()
  @IsNotEmpty()
  /*readonly*/ date!: number
  @IsString()
  @Matches(/(?:UTC|[a-z]*?\/[a-z]+)/i, {
    message:
      'Only valid Olson TimeZones names are allowed, e.g. Europe/Berlin or UTC',
  })
  /*readonly*/ timezone!: string
  @IsArray()
  @ValidateNested({ each: true })
  @Type(() => CreateEvent)
  /*readonly*/ events!: CreateEvent[]
}
