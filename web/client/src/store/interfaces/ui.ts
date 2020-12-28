import { ScConstruction } from './types';

export enum RunningOperation {
  None,
  Search,
  Generate
}

export namespace kb {
  
  export interface State {
    running: RunningOperation,
    construction: ScConstruction,
  }

  export const _initState: State = {
    running: RunningOperation.None,
    construction: null,
  }
} // namespace kb

export enum Mode {
  Initializing,
  MainUI,
}

export interface State {
  mode: Mode,
  initMessage: string,
  kb: kb.State,
}

export const _initState: State = {
  mode: Mode.Initializing,
  initMessage: '',
  kb: kb._initState,
}