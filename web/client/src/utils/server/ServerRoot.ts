
import { ScNet } from '@ostis/sc-core';

import { ServerKeynodes } from './ServerKeynodes';
import { ServerTemplates } from './ServerTemplates';

import * as store from '../../store';

import * as redux from 'redux';
import { net } from '../../store';


export class ServerRoot {
  private _client: ScNet = null;
  private _url: string = '';
  private _store: redux.Store<store.Store> = null;

  private _serverKeynodes: ServerKeynodes = null;
  private _serverTemplates: ServerTemplates = null;

  /** Constructor
   * @param url URL to websocket service
   * @param store Redux store instance
   */
  constructor(url: string, store: redux.Store<store.Store>) {
    this._url = url;
    this._store = store;
  }

  public Start() {
    this.NotifyChangeInitState('Connecting');
    this.NotifyChangeNetworkState(store.net.State.Connecting);
    this._client = new ScNet(this._url, this.OnConnected.bind(this), this.OnDisconnected.bind(this), this.OnError.bind(this));

    this._serverKeynodes = new ServerKeynodes(this._client);
    this._serverTemplates = new ServerTemplates(this._client, this._serverKeynodes);

    this._store.dispatch(store.actions.serv.Init({
      sc: this
    }));
  }

  private NotifyChangeInitState(text: string) {
    this._store.dispatch(store.actions.ui.ChangeInitMessage(text));
  }

  private NotifyChangeNetworkState(newState: net.State) {
    this._store.dispatch(store.actions.net.changeNetState(newState));
  }

  private OnConnected() {
    this.NotifyChangeNetworkState(net.State.Connected);
    this.Initialize();
  }

  private OnDisconnected() {
    this.NotifyChangeNetworkState(net.State.Disconnected);
  }

  private OnError() {

  }

  private async Initialize(): Promise<boolean> {
    const self = this;

    async function delay() {
      return new Promise<void>(resolve => {
        setTimeout(resolve, 500)
      })
    }

    this.NotifyChangeInitState('Initialize keynodes');
    let result: boolean = await self._serverKeynodes.Initialize();
    await delay();

    this.NotifyChangeInitState('Initialize templates');
    result = await self._serverTemplates.Initialize();
    await delay();

    return new Promise<boolean>(function (resolve) { 
      self._store.dispatch(store.actions.ui.ChangeUIMode(store.ui.Mode.MainUI));
      resolve(result);
    });
  }

  public get keynodes() : ServerKeynodes {
    return this._serverKeynodes;
  }

  public get templates() : ServerTemplates {
    return this._serverTemplates;
  }

  public get client() : ScNet {
    return this._client;
  }
};