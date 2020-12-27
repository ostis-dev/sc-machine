import * as React from "react";
import { BasePage } from './BasePage';
import { connect } from 'react-redux';

import * as store from '../store/store';

export interface DashboardProps {
}

export interface DashboardState {
}


function mapStateToProps(state: store.Store): any {
  return {
  };
}

class DashboardImpl extends React.Component<DashboardProps, DashboardState> {

  render() {

    return (
      <BasePage title='Dashboard'>
      </BasePage>
    );
  }
}

export const Dashboard = connect(
  mapStateToProps
)(DashboardImpl);