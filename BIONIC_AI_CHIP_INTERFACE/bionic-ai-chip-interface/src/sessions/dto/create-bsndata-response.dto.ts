export class CreateBSNDataResponseDTO {
  readonly date_utc!: string
  readonly timezone!: string
  readonly task!: string
  readonly events_in_session!: number
  readonly documents_stored!: number
}
