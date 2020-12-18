import { ScTemplateResult} from '@ostis/sc-core';

export interface ScConstruction {
  timestamp: number,
  elements: ScTemplateResult[],
}

export function CreateScConstruction(elements: ScTemplateResult[]): ScConstruction {
  return {
    timestamp: Date.now(),
    elements: elements,
  };
}