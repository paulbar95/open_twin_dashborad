import React from 'react';
import Session from './Session/Session';

const LocalSessionService = ({ serviceNumber, sessions }) => {
    return (
        <div className="card">
            <div className="card-header">Local Session Service #{serviceNumber}</div>
            <div className="card-body">
                <div className="inner-grid-container">
                    {sessions.map((session, index) => (
                        <Session key={index} user={session.user} accessedLDS={session.accessedLDS} />
                    ))}
                </div>
            </div>
        </div>
    );
};

export default LocalSessionService;
