import BSNEvent from './bsnevent.interface'

export default interface BSNSession {
  worker_id?: string
  timezone?: string
  task: string
  date: string | number
  events: BSNEvent[]
}
