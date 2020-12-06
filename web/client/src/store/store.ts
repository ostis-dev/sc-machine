import { ServerRoot, ServerTemplates } from '../utils/server';

export enum NetworkState {
  Disconnected,
  Connecting,
  Connected,
};

export enum UIMode {
  Initializing,
  MainUI,
};

export interface Services {
  root: ServerRoot,
  templates: ServerTemplates,
}

export interface UIState {
  mode: UIMode,
  initMessage: string,
}

export interface Store {
  net: NetworkState,
  services: Services
  ui: UIState
};

/// Initial states
export const servicesInit = {
  root: null,
  templates: null,
};

export const uiInit = {
  mode: UIMode.Initializing,
  initMessage: ''
}

export const storeInitialState: Store = {
  net: NetworkState.Disconnected,
  services: servicesInit,
  ui: uiInit,
};