import BSNCategory from '../interfaces/bsncategory.interface'
import BSNDatum from '../interfaces/bsndatum.interface'
import BSNEvent from '../interfaces/bsnevent.interface'
import BSNSession from '../interfaces/bsnsession.interface'

class Datum implements BSNDatum {
  readonly name!: string

  readonly offset?: number

  readonly sampling_rate?: number

  readonly number_of_samples?: number
  // TODO distinguish between different data types using validators
  // https://github.com/typestack/class-transformer#providing-more-than-one-type-option
  // TODO distinguish between different data types using validators
  // https://github.com/typestack/class-transformer#providing-more-than-one-type-option

  readonly samples!: Array<number[] | number | string> | number | string
}

class Category implements BSNCategory {
  readonly name!: string

  readonly data!: Datum[]
}

class Event implements BSNEvent {
  readonly time!: number

  readonly level!: number

  readonly categories!: Category[]
}

export class BSNDataDTO implements BSNSession {
  readonly worker_id!: string
  readonly task!: string
  readonly date!: string
  readonly events!: Event[]
}
