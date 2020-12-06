import * as React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';
import { createMuiTheme, MuiThemeProvider } from '@material-ui/core';

import { DrawerItem } from './ui/DrawerItem';

import * as store from './store';
import * as ui from '@material-ui/core';

import { NetworkState, UIState, Store, UIMode, storeInitialState } from './store';

const theme = createMuiTheme({
  palette: {
    primary: {
      main: '#000000',
    },
    secondary: {
      main: '#f44336',
    },
  },
  props: {
    MuiButtonBase: {
      disableRipple: true
    }
  }
});

interface AppContainerProps {
  store?: Store
};

function mapStateToProps(state: Store): any {
  return {
    store: state
  };
};

export class AppContainerImpl extends React.Component<AppContainerProps, any> {
  
  constructor(props: Readonly<AppContainerProps>) {
    super(props);    
  }

  private renderMainUI() : React.ReactNode {
    const isConnected: boolean = (this.props.store.net === store.NetworkState.Connected);
    return (
      <div className='body-content'>
        <ui.CssBaseline />
        <nav>
          <ui.Hidden xsDown implementation='css'>
            <ui.Drawer
              variant='permanent'
              open
              className='drawer'
              classes={{ 
                paper: 'paper'
              }}>
                
                <div className='drawer-status'>
                  <ui.Icon className={isConnected ? 'ok' : 'error'}>
                    {isConnected ? 'wifi' : 'wifi_off'}
                  </ui.Icon>
                </div>
                <ui.Divider/>

                <ui.List>
                  <DrawerItem to='/' key='dashboard' title='Dashboard' icon='dashboard'></DrawerItem>
                  <DrawerItem to='/' key='knowledgebase' title='Knowledge base' icon='create'></DrawerItem>
                </ui.List>
            </ui.Drawer>
          </ui.Hidden>
        </nav>        
      </div>
    );
  }

  private renderLoader(msg: string) : React.ReactNode {
    return (
      <div className='loader-container'>
        <div className='loader-vertical-center'>
          <div className='loader'></div>
          <div className='loader-text'>{ msg }</div>
        </div>
      </div>
    );
  }
  
  render() {
    const self = this;
    const uiState: UIState = this.props.store.ui;

    function renderImpl() : React.ReactNode {
      if (uiState.mode === UIMode.MainUI)
        return self.renderMainUI();
      
      return self.renderLoader(uiState.initMessage);
    }   

    return (
      <MuiThemeProvider theme={theme}>
        { renderImpl() }
      </MuiThemeProvider>
    );
  }
}

export const AppContainer = withRouter(
  connect(mapStateToProps)(AppContainerImpl) as any);