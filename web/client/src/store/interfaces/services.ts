import { ServerRoot, ServerTemplates } from '../../utils/server';

export interface Services {
  sc: ServerRoot,
}

export const _initServices: Services = {
  sc: null
};
