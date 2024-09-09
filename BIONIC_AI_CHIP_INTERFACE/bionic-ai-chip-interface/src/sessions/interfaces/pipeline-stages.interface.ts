export interface PipelineStages {
  readonly match: {
    [key: string]: unknown
  }
  readonly limit: number
  readonly skip: number
}
