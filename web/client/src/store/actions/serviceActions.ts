import { Action } from './baseAction';
import { Services } from '../store';

export namespace Type {
  export const Init = 'SERVICES_INIT';
}

export function Init(services: Services): Action<Services> {
  return {
    type: Type.Init,
    payload: services
  };
}
