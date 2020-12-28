import { net, serv, ui } from './interfaces';

export interface Store {
  net: net.State,
  services: serv.Services
  ui: ui.State
};

export const storeInitialState: Store = {
  net: net.State.Disconnected,
  services: serv._initServices,
  ui: ui._initState,
};