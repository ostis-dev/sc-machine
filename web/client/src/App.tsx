import * as React from 'react';
import { connect } from 'react-redux';
import { withRouter, Route, Switch } from 'react-router-dom';
import { createMuiTheme, MuiThemeProvider } from '@material-ui/core';

import { DrawerItem } from './ui/DrawerItem';
import { Dashboard } from './ui/Dashboard';
import { KnowledgeBase } from './ui/KnowledgeBase';

import * as store from './store';
import * as ui from '@material-ui/core';

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
  store?: store.Store
};

function mapStateToProps(state: store.Store): any {
  return {
    store: state
  };
};

export class AppContainerImpl extends React.Component<AppContainerProps, any> {

  private renderMainUI(): React.ReactNode {
    const isConnected: boolean = (this.props.store.net === store.net.State.Connected);
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
              <ui.Divider />
              <ui.List>
                <DrawerItem to='/' key='dashboard' title='Dashboard' icon='dashboard'></DrawerItem>
                <DrawerItem to='/kb' key='knowledgebase' title='Knowledge base' icon='create'></DrawerItem>
              </ui.List>
            </ui.Drawer>
          </ui.Hidden>
        </nav>
        <main>
          <Switch>
            <Route exact path='/' component={Dashboard} />
            <Route path='/kb' component={KnowledgeBase} />
          </Switch>
        </main>
      </div>
    );
  }

  private renderLoader(msg: string): React.ReactNode {
    return (
      <div className='loader-container'>
        <div className='loader-vertical-center'>
          <div className='loader'></div>
          <div className='loader-text'>{msg}</div>
        </div>
      </div>
    );
  }

  render() {
    const self = this;
    const uiState: store.ui.State = this.props.store.ui;

    function renderImpl(): React.ReactNode {
      if (uiState.mode === store.ui.Mode.MainUI)
        return self.renderMainUI();

      return self.renderLoader(uiState.initMessage);
    }

    return (
      <MuiThemeProvider theme={theme}>
        { renderImpl()}
      </MuiThemeProvider>
    );
  }
}

export const AppContainer = withRouter(
  connect(mapStateToProps)(AppContainerImpl) as any);