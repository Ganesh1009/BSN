import BSNCategory from './bsncategory.interface'

export default interface BSNEvent {
  time: number
  level: number
  categories: BSNCategory[]
}
