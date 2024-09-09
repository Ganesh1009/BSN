export default interface BSNDatum {
  name: string
  offset?: number
  sampling_rate?: number
  number_of_samples?: number
  samples: Array<number[] | number | string> | number | string
}
