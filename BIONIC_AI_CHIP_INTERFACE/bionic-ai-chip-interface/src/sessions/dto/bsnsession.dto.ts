class Session {
  readonly id!: number
  readonly date!: string
  readonly task!: string
  readonly number_of_events!: number
  readonly average_level!: number
}

export class BSNSessionDTO {
  readonly worker_id!: string
  readonly sessions!: Session[]
}
