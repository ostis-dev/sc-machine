import { Action } from './baseAction';
import * as serv from '../interfaces/services';

export namespace Type {
  export const Init = 'SERVICES_INIT';
}

export function Init(services: serv.Services): Action<serv.Services> {
  return {
    type: Type.Init,
    payload: services
  };
}
