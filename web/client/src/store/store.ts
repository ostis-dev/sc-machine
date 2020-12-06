import { ServerRoot, ServerTemplates } from '../utils/server';

export enum NetworkState {
  Disconnected,
  Connecting,
  Connected,
};

export interface Services {
  root: ServerRoot,
  templates: ServerTemplates,
}

export interface Store {
  net: NetworkState,
  services: Services
};

/// Initial states
export const storeInitialState: Store = {
  net: NetworkState.Disconnected,
  services: {
    root: null,
    templates: null,
  }
};