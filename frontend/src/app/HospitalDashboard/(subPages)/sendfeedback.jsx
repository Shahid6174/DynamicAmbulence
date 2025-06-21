'use client';

import { useState } from 'react';
import { Card, Form, Button, Alert } from 'react-bootstrap';

export default function SendFeedback() {
  const [form, setForm] = useState({
    type: 'Incident',
    message: '',
    reporter: '',
    date: new Date().toISOString().slice(0, 10),
  });

  const [submitted, setSubmitted] = useState(false);

  const handleChange = (e) => {
    setForm({ ...form, [e.target.name]: e.target.value });
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    // TODO: Send to API
    setSubmitted(true);
    setTimeout(() => setSubmitted(false), 3000);
    setForm({ ...form, message: '', reporter: '' });
  };

  return (
    <div className="container-fluid py-4">
      {/* Header */}
      <div className="d-flex justify-content-between align-items-center mb-4">
        <div>
          <h4 className="fw-bold text-primary mb-1">
            <i className="bi bi-chat-left-dots-fill me-2"></i>Send Feedback
          </h4>
          <p className="text-muted small mb-0">
            Report incidents, delays, or concerns to the main admin team
          </p>
        </div>
      </div>

      {/* Form */}
      <Card className="shadow-sm">
        <Card.Body>
          {submitted && <Alert variant="success">Feedback submitted ✅</Alert>}
          <Form onSubmit={handleSubmit}>
            <Form.Group className="mb-3">
              <Form.Label>Feedback Type</Form.Label>
              <Form.Select name="type" value={form.type} onChange={handleChange}>
                <option>Incident</option>
                <option>Delay</option>
                <option>System Issue</option>
                <option>Suggestion</option>
                <option>Other</option>
              </Form.Select>
            </Form.Group>

            <Form.Group className="mb-3">
              <Form.Label>Feedback Description</Form.Label>
              <Form.Control
                as="textarea"
                rows={4}
                name="message"
                value={form.message}
                onChange={handleChange}
                placeholder="Describe the issue, suggestion, or delay..."
                required
              />
            </Form.Group>

            <Form.Group className="mb-3">
              <Form.Label>Your Name (Optional)</Form.Label>
              <Form.Control
                type="text"
                name="reporter"
                value={form.reporter}
                onChange={handleChange}
                placeholder="e.g., Driver ID, Operator Name"
              />
            </Form.Group>

            <Form.Group className="mb-4">
              <Form.Label>Date</Form.Label>
              <Form.Control
                type="date"
                name="date"
                value={form.date}
                onChange={handleChange}
              />
            </Form.Group>

            <Button type="submit" variant="warning">
              <i className="bi bi-send-fill me-1"></i>Submit Feedback
            </Button>
          </Form>
        </Card.Body>
      </Card>
    </div>
  );
}
