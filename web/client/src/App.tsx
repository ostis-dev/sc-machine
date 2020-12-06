import * as React from 'react';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';
import { createMuiTheme, MuiThemeProvider } from '@material-ui/core';

import { Store } from './store';

const theme = createMuiTheme({
  palette: {
    primary: {
      main: '#000000',
    },
    secondary: {
      main: '#f44336',
    },
  },
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
  render() {
    return (
      <MuiThemeProvider theme={theme}>
        <p>Hello World</p>
      </MuiThemeProvider>
    );
  }
}

export const AppContainer = withRouter(
  connect(mapStateToProps)(AppContainerImpl) as any);